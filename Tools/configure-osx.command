cd "$(dirname $0)"
python -m dreemchest configure macos --source ../Source --output ../Projects/MacOS --no-pch --no-tests --no-relight --cpp 11 --prefix-path ../Build/Dependencies