cd "$(dirname $0)"
python -m dreemchest configure windows --source ../Source --output ../Projects/Windows --no-tests --no-relight --cpp 11 --prefix-path ../Build/Dependencies/Windows --no-composer
pause