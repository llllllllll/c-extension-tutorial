set -e
echo "provisioning c_extension_tutorial VM"

dnf builddep -y python3
dnf install -y gdb

echo "c_extension_tutorial VM provisioned successfully"
