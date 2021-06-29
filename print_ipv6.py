import subprocess

ip_addresses= subprocess.check_output("ip -6 addr show wwan0 | grep -oP '(?<=inet6\s)[\da-f:]+'", shell=True)
ip_addresses=ip_addresses.decode('ascii')
ip_addresses=ip_addresses.splitlines()
print(ip_addresses[1])