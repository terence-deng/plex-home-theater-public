rem Create CA
makecert.exe -r -pe -n "CN=\"Plex, Inc. Test CA\"" -ss CA -sr CurrentUser -a sha1 -len 2048 -sky signature -sv PlexTestCA.pvk PlexTestCA.cer

rem Create code-signing certificate
makecert.exe -pe -n "CN=\"Plex, Inc. Test\"" -a sha1 -len 2048 -sky signature -ic PlexTestCA.cer -iv PlexTestCA.pvk -sv PlexTestSPC.pvk PlexTestSPC.cer

rem Create pfx files
pvk2pfx.exe -f -pvk PlexTestSPC.pvk -spc PlexTestSPC.cer -pfx PlexTestSPC.pfx
pvk2pfx.exe -f -pvk PlexTestCA.pvk -spc PlexTestCA.cer -pfx PlexTestCA.pfx
