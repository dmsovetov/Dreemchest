cd "$(dirname $0)"
python -m Pygling configure --output ../projects --source ../src --name dreemchest —platform iOS --renderer=OpenGL --sound=OpenAL --threads=Posix --io=yes --network=yes