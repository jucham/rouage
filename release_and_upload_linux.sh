echo "build for architecture $2 bits"
rm -rf rouage-linux-*
mkdir rouage-linux-$1-$2
mkdir rouage-linux-$1-$2/lib
cp libs/cegui/lib/linux/$2/* rouage-linux-$1-$2/lib
cp libs/sdl/lib/linux/$2/* rouage-linux-$1-$2/lib
cp -R Rouage RouageServer client.cfg data-clean lib rouage-linux-$1-$2
mv rouage-linux-$1-$2/data-clean rouage-linux-$1-$2/data
tar -cvzf rouage-linux-$1-$2.tgz rouage-linux-$1-$2
ftp -n <<EOF
passive
open server.local
user rouage rouage
put rouage-linux-$1-$2.tgz rouage-linux-$1-$2.tgz
EOF
