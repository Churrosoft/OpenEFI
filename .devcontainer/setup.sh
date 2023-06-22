## Install rustup and common components
curl https://sh.rustup.rs -sSf | sh -s -- -y 
rustup install nightly
rustup default nightly
rustup target add thumbv7em-none-eabihf
rustup component add llvm-tools-preview
 
cargo install cargo-binutils cargo-embed cargo-flash cargo-expand
## setup and install oh-my-zsh
sh -c "$(curl -fsSL https://raw.githubusercontent.com/robbyrussell/oh-my-zsh/master/tools/install.sh)"
cp -R /root/.oh-my-zsh /home/$USERNAME
cp /root/.zshrc /home/$USERNAME
sed -i -e "s/\/root\/.oh-my-zsh/\/home\/$USERNAME\/.oh-my-zsh/g" /home/$USERNAME/.zshrc
chown -R $USER_UID:$USER_GID /home/$USERNAME/.oh-my-zsh /home/$USERNAME/.zshrc