REM Update the main repo
git checkout main
git fetch
git pull

REM Init the subrepos
git submodule update --init --recursive
git submodule foreach --recursive "git checkout master"
git submodule foreach --recursive "git checkout main"

REM Update the subrepos
git submodule foreach --recursive "git pull"