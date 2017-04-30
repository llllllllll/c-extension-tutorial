echo "provisioning c_extension_tutorial VM"

dnf install -y dnf-plugins-core
dnf builddep -y python3

echo "c_extension_tutorial VM provisioned successfully"
