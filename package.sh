#/bin/bash

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

# Get the python version
PYTHON_VERSION=


export CIBW_BUILD="cp312-manylinux_x86_64"  # Since this is for testing before CI, only one python version to be tested
#export CIBW_BUILD="cp3*-manylinux_x86_64"

#export CIBW_REPAIR_WHEEL_COMMAND=$(cat <<-EOM
#  cat build/src/poem/version.h
#  cat src/pypoem/_version.py
#EOM
#)

pip uninstall -y pypoem
rm linuxwheels/*
cibuildwheel --print-build-identifiers
cibuildwheel --output-dir linuxwheels --platform linux --debug-traceback --allow-empty

OUTPUT=$(echo `ls linuxwheels`)
pip install linuxwheels/$OUTPUT
