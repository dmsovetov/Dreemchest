cd "$(dirname $0)"
cd ../../yap
yap MacOS --source ../dreemchest/src --output ../projects/dreemchest.mac --name dreemchest --renderer=OpenGL --sound=OpenAL --threads=Posix