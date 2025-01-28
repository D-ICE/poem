
'which ssh-agent || ( apt update -y && apt install -y openssh-client )'
eval $(ssh-agent -s)
mkdir -p ~/.ssh
echo "$GITLAB_CI_SSH_PRIVATE_KEY" | tr -d '\r' > ~/.ssh/id_rsa
chmod 700 ~/.ssh
chmod 700 ~/.ssh/id_rsa
ssh-add
echo "$GITLAB_CI_SSH_KNOWN_HOST" | tr -d '\r' > ~/.ssh/known_hosts
apt update -y && apt install -y uuid-dev
