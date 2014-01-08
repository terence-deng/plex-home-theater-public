/////////////////////////////////////////////////////////////////////////////////////////
// CProfiler Class Definition
/////////////////////////////////////////////////////////////////////////////////////////
// Author : LongChair - 2013
// Description :	When trying to profile Plex on RPi, the lack of proper tools to 
//					efficiently get some figures on timing functions led me to create
//					this utiliry class. It will just time functions and subfunctions 
//					when the macros are used and will then report the timing into 
//					an hierachically organised text file
/////////////////////////////////////////////////////////////////////////////////////////

#include "CProfiler.h"
#include "threads/Thread.h"
#include "system.h"



CProfiler g_Profiler;		// global Profiler class
CStopWatch g_ProfileWatch;	// Stop watch used for timing


///////////////////////////////////////////////////////////////////////
// CProfiledFunction Class methods
///////////////////////////////////////////////////////////////////////
CProfiledFunction *CProfiledFunction::FindChild(CStdString aName)
{
    for (std::list<CProfiledFunction*>::iterator it = ChildFunctions.begin();it !=ChildFunctions.end();++it)
    {
        if ((*it)->GetName()==aName) return (*it);
    }

    return NULL;
}

 CProfiledFunction *CProfiledFunction::AddChildFunction(CProfiledFunction *pFunc)
 {
     // Update the parent
     pFunc->SetParent(this);

     // add it to child functions list
     ChildFunctions.push_back(pFunc);
     return pFunc;
 }

 void CProfiledFunction::Start()
 {
     m_StartTime = GetTime();
     m_bStarted = true;
     m_NumHits++;
 }

 void CProfiledFunction::End()
 {
     if (m_bStarted)
     {
         // End All Sub Functions
         for (std::list<CProfiledFunction*>::iterator it=ChildFunctions.begin(); it!=ChildFunctions.end(); ++it)
         {
             (*it)->End();
         }

         // update the end time info
         m_EndTime = GetTime();
         m_TotalTime += (m_EndTime - m_StartTime);
         m_bStarted = false;
         PROFILE_DEBUG("ending start=%2.4f, end=%2.4f, total = %2.4f",m_StartTime,m_EndTime,m_TotalTime);
     }
 }

 void CProfiledFunction::Clear()
 {
     for (std::list<CProfiledFunction*>::iterator it=ChildFunctions.begin(); it!=ChildFunctions.end(); ++it)
     {
         delete (*it);
     }

     ChildFunctions.clear();
 }

 void CProfiledFunction::PrintStats(FILE* File,int Level)
 {
     // Print the current function Stats
	 CStdString sPad;
     for (int i=0;i<Level;i++) sPad += "  | ";

	 // try to get the parent total time
	 float ParentTime = 0;
	 if (GetParent()) ParentTime = GetParent()->GetTotalTime();

	 // compute our percentage compared to parent totaltime
	 float Percentage = 0;
	 if (ParentTime) Percentage = (m_TotalTime * 100.0f) / ParentTime; 

	 // log information onto file
	 CStdString sLine;
     sLine.Format("%s% 3d%%,%5d hit(s), % 2.3fs avg:%2.3fs- %s\n", sPad.c_str(),((int)Percentage),m_NumHits, m_TotalTime,m_TotalTime / m_NumHits, m_Name.c_str());
     fputs(sLine.c_str(),File);

     // iterate on childs
     for (std::list<CProfiledFunction*>::iterator it=ChildFunctions.begin(); it!=ChildFunctions.end(); ++it)
     {
         (*it)->PrintStats(File,Level+1);
     }
 }

///////////////////////////////////////////////////////////////////////
// CProfiler Class methods
///////////////////////////////////////////////////////////////////////

CProfiler::CProfiler() 
{ 
    m_Enabled = false;
    m_bStopWatchStarted = false;
}

CProfiler::~CProfiler()
{
	Reset();
}


void CProfiler::StartFunction(CStdString FunctionName)
{
	if (!m_Enabled) return;

    // Start Stopwatch if not already done
    if (!m_bStopWatchStarted)
    {
        g_ProfileWatch.StartZero();
        m_bStopWatchStarted = true;
    }

    int ThreadID = CThread::GetCurrentThreadId();

    // check if we have a root for this thread
    std::map<int,CProfiledFunction*>::iterator it = m_Root.find(ThreadID);
    if (it==m_Root.end())
    {
        CStdString RootName;
        RootName.Format("Root %X",ThreadID);
        m_Root[ThreadID] = new CProfiledFunction(RootName);
        m_CurrentFunction[ThreadID] = m_Root[ThreadID];
    }

    CProfiledFunction *CurrentFunction = m_CurrentFunction[ThreadID];

    PROFILE_DEBUG("%s - Entering",FunctionName.c_str())

    //we look in child functions if we already have it
    CProfiledFunction *pChildFunction = CurrentFunction->FindChild(FunctionName);

    // if we don't have it, then create one as a child
    if (!pChildFunction)
    {
        PROFILE_DEBUG("Creating Child function of %s -> %s",CurrentFunction->GetName().c_str(),FunctionName.c_str())
        pChildFunction = CurrentFunction->AddChildFunction(FunctionName);
    }
    else PROFILE_DEBUG("Child function found","")

    // now current function is child function
    CurrentFunction = pChildFunction;

    // now we update its info
    CurrentFunction->Start();

    m_CurrentFunction[ThreadID] = CurrentFunction;
    PROFILE_DEBUG("%s - Exiting",FunctionName.c_str())
}

void CProfiler::EndFunction(CStdString FunctionName)
{
	if (!m_Enabled) return;

    int ThreadID = CThread::GetCurrentThreadId();
    CProfiledFunction *CurrentFunction = m_CurrentFunction[ThreadID];

    PROFILE_DEBUG("%s - Entering",FunctionName.c_str())
    // We close eventually all subfunctions
    while (CurrentFunction->GetName()!=FunctionName)
    {
        PROFILE_DEBUG("Current function (%s) is not the one Ending (%s)",CurrentFunction->GetName().c_str(),FunctionName.c_str())
        // if the name is not matching, then we have exit the current function without calling endFunction
        // therfore we close current function and go up
        CurrentFunction->End();
        CurrentFunction = CurrentFunction->GetParent();
    }

    // and close the current function
    if (CurrentFunction)
    {
        PROFILE_DEBUG("Ending function %s",CurrentFunction->GetName().c_str())
        CurrentFunction->End();

        // now pop back to the parent
        CurrentFunction = CurrentFunction->GetParent();
        PROFILE_DEBUG("Current Function now is %s",CurrentFunction->GetName().c_str())
    }
    PROFILE_DEBUG("%s - Exiting",FunctionName.c_str())

    m_CurrentFunction[ThreadID] = CurrentFunction;
}

 void CProfiler::PrintStats()
{
    FILE* File;
	CStdString sLine;
	SYSTEMTIME time;
    GetLocalTime(&time);

	CStdString OutFileName;
    //OutFileName.Format("Profile_%02d_%02d_%02d.txt",time.wHour, time.wMinute, time.wSecond);
    OutFileName.Format("Profile.txt",time.wHour, time.wMinute, time.wSecond);

    File = fopen64_utf8(OutFileName.c_str(),"wb");
    if (File)
	{
        PROFILE_DEBUG("Outputing stats in %s",OutFileName.c_str())

        sLine.Format("Profiler results dump for %d threads :\n",m_Root.size());
        fputs(sLine.c_str(),File);

		// now print individual theards results
		int iCount =1;
		for (std::map<int,CProfiledFunction*>::iterator it=m_Root.begin(); it!=m_Root.end(); ++it)
		{
            sLine.Format("-----------Thread %2d (%10X)-----------\n",iCount,it->first);
            fputs(sLine.c_str(),File);

            it->second->PrintStats(File,0);

            sLine = "--------------------------------------------\n";
            fputs(sLine.c_str(),File);

			iCount++;
		}

        fclose(File);
	}
    else PROFILE_DEBUG("Failed to open %s",OutFileName.c_str())

}

 void CProfiler::Reset()
 {
     for (std::map<int,CProfiledFunction*>::iterator it=m_Root.begin(); it!=m_Root.end(); ++it)
     {
         delete it->second;
     }

     m_Root.clear();
	 m_CurrentFunction.clear();
 }

