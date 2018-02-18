if [ -f "log" ];
then
rm log
fi

if [ -f "dump" ];
then
rm dump
fi

if [ -d "intermediateGfs" ]
then
rm -r intermediateGfs
fi

if [ -d "intermediateBview" ]
then
rm -r intermediateBview
fi

mkdir intermediateGfs
mkdir intermediateBview

qcc -O2 -Wall -grid=octree $file.c -o $file -lm \
    -L$BASILISK/gl -lglutils -lfb_glx -lGLU -lGLEW -lGL -lX11 -lm

#qcc -O2 -Wall $file.c -o $file -lm \
#    -L$BASILISK/gl -lglutils -lfb_osmesa -lGLU -lOSMesa -lm
