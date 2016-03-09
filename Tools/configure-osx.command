cd "$(dirname $0)"
python -m Pygling configure --output ../projects --source ../src --name dreemchest —platform MacOS --renderer=OpenGL --sound=OpenAL --threads=Posix --io=yes --network=yes