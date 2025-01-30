yum install -y openssh-clients &&
eval $(ssh-agent -s) &&
mkdir -p ~/.ssh &&
echo "$GITLAB_CI_SSH_PRIVATE_KEY" | tr -d '\r' > ~/.ssh/id_rsa &&
chmod 700 ~/.ssh &&
chmod 700 ~/.ssh/id_rsa &&
echo "$GITLAB_CI_SSH_KNOWN_HOST" | tr -d '\r' > ~/.ssh/known_hosts &&
yum install -y graphviz