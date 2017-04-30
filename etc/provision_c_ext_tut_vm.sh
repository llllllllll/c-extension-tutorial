set -e
echo "provisioning c_extension_tutorial VM"

dnf builddep -y python3

echo "c_extension_tutorial VM provisioned successfully"
