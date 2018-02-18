if [ ! -d "intermediateGfs" ]
then
mkdir intermediateGfs
fi

if [ ! -d "intermediateBview" ]
then
mkdir intermediateBview
fi

qcc -O2 -Wall -grid=octree $file.c -o $file -lm \
    -L$BASILISK/gl -lglutils -lfb_glx -lGLU -lGLEW -lGL -lX11 -lm

#qcc -O2 -Wall $file.c -o $file -lm \
#    -L$BASILISK/gl -lglutils -lfb_osmesa -lGLU -lOSMesa -lm
