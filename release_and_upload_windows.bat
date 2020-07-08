del *.zip
for /D %%f in (rouage-windows-*) do rmdir %%f /S /Q
mkdir rouage-windows-%1
xcopy /F /Q Rouage.exe rouage-windows-%1
xcopy /F /Q RouageServer.exe rouage-windows-%1
xcopy /F /Q client.cfg rouage-windows-%1
xcopy /F /Q SDL2.dll rouage-windows-%1
xcopy /F /Q freetype6.dll rouage-windows-%1
xcopy /F /Q zlib1.dll rouage-windows-%1
xcopy /F /Q libCEGUIBase-0.dll rouage-windows-%1
xcopy /F /Q libCEGUIOpenGLRenderer-0.dll rouage-windows-%1
xcopy /F /Q libfreetype.dll rouage-windows-%1
xcopy /F /Q libpcre.dll rouage-windows-%1
xcopy /F /Q libglew.dll rouage-windows-%1
xcopy /F /Q libexpat.dll rouage-windows-%1
xcopy /F /Q libCEGUICoreWindowRendererSet.dll rouage-windows-%1
xcopy /F /Q libCEGUIExpatParser.dll rouage-windows-%1
xcopy /F /Q libCEGUISTBImageCodec.dll rouage-windows-%1
xcopy /F /Q libpng13.dll rouage-windows-%1
xcopy /F /Q libgcc_s_dw2-1.dll rouage-windows-%1
xcopy /F /Q libstdc++-6.dll rouage-windows-%1
mkdir rouage-windows-%1\data
xcopy /F /Q data-clean rouage-windows-%1\data /Y /E
"C:\Program Files (x86)\7-Zip\7z.exe" a rouage-windows-%1.zip rouage-windows-%1
rem ftp -s:upload.scr
    