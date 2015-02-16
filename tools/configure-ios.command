cd "$(dirname $0)"
cd ../scripts
yap iOS --source ../dreemchest/src --output ../projects/dreemchest.ios --name dreemchest --renderer=OpenGL --sound=OpenAL --threads=Posix