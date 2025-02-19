#/bin/bash

cd ..

# Here, we need "id_rsa_ci_dice" and "known_hosts_ci_dice" fles to be present in  ~/.ssh/ci-dice/
# with private ssh key and known host of CI
# Please ask francois.rongere@dice-engineering.com if needed

export SSH_PRIVATE_KEY=`cat ~/.ssh/ci-dice/id_rsa_ci_dice`
export SSH_KNOWN_HOST=`cat ~/.ssh/ci-dice/known_hosts_ci_dice`

export CIBW_BEFORE_ALL=$(cat <<-EOM
  yum update
  yum install -y openssh-clients
  eval $(ssh-agent -s)
  mkdir -p ~/.ssh
  echo "$SSH_PRIVATE_KEY" | tr -d '\r' > ~/.ssh/id_rsa
  chmod 700 ~/.ssh
  chmod 700 ~/.ssh/id_rsa
  echo "$SSH_KNOWN_HOST" | tr -d '\r' > ~/.ssh/known_hosts
EOM
)

#export CIBW_BUILD="cp312-manylinux_x86_64"  # Since this is for testing before CI, only one python version to be tested
#export CIBW_BUILD="cp3*-manylinux_x86_64"

# Targeting the current environment Python version
export CIBW_BUILD=`python -c 'import sys; version=sys.version_info[:3]; print("cp{0}{1}-manylinux_x86_64".format(*version))'`

#export CIBW_REPAIR_WHEEL_COMMAND=$(cat <<-EOM
#  cat build/src/poem/version.h
#  cat src/pypoem/_version.py
#EOM
#)

pip install cibuildwheel
rm -rf linuxwheels
cibuildwheel --print-build-identifiers
cibuildwheel --output-dir linuxwheels --platform linux --debug-traceback --allow-empty

pip uninstall -y pypoem
rm -rf tests/pypoem/pypoem
rm tests/pypoem/*.nc
OUTPUT=$(echo `ls linuxwheels`)
pip install linuxwheels/$OUTPUT
rm -rf linuxwheels

cd scripts
