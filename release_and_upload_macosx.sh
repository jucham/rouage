rm -rf rouage-macosx-*
mkdir rouage-macosx-$1
cp -R release-skeleton/macosx/* rouage-macosx-$1
cp Rouage rouage-macosx-$1/Rouage.app/Contents/MacOS
cp RouageServer rouage-macosx-$1/RouageServer.app/Contents/MacOS
cp -R data-clean/* rouage-macosx-$1/Rouage.app/Contents/Resources/data
tar -cvzf rouage-macosx-$1.tgz rouage-macosx-$1
ftp -u ftp://rouage:rouage@server.local/rouage-macosx-$1.tgz rouage-macosx-$1.tgz