#!/usr/bin/env python3

import json
import os
import subprocess
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
TF_DIR = os.environ.get("TF_DIR", os.path.abspath(os.path.join(SCRIPT_DIR, "..", "..")))
SSH_USER = os.environ.get("ANSIBLE_SSH_USER", "devops")


def terraform_vms():
    try:
        out = subprocess.check_output(
            ["terraform", "output", "-json", "vms"],
            cwd=TF_DIR,
            stderr=subprocess.PIPE,
        )
    except FileNotFoundError:
        sys.stderr.write("error: terraform not found in PATH\n")
        return {}
    except subprocess.CalledProcessError as exc:
        sys.stderr.write(
            "error: `terraform output -json vms` failed. "
            "Did you run `terraform apply`?\n"
            + exc.stderr.decode(errors="replace")
        )
        return {}
    return json.loads(out or b"{}") or {}


def build_inventory():
    inv = {
        "_meta": {"hostvars": {}},
        "server": {"hosts": []},
        "agent": {"hosts": []},
        "k3s_cluster": {"children": ["server", "agent"]},
    }

    hosts = {}
    for name, info in terraform_vms().items():
        ip = (info or {}).get("ip")
        if not ip:
            sys.stderr.write(f"warning: {name} has no ip yet, skipping\n")
            continue
        hosts[name] = ip

    if not hosts:
        return inv

    names = sorted(hosts)
    server = os.environ.get("K3S_SERVER_HOST")
    if server not in hosts:
        server = names[0]

    for name in names:
        inv["_meta"]["hostvars"][name] = {
            "ansible_host": hosts[name],
            "ansible_user": SSH_USER,
        }
        group = "server" if name == server else "agent"
        inv[group]["hosts"].append(name)

    return inv


def main():
    if "--host" in sys.argv:
        print(json.dumps({}))
        return
    print(json.dumps(build_inventory(), indent=2))


if __name__ == "__main__":
    main()
