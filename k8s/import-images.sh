#!/usr/bin/env bash
set -euo pipefail

NODES=(rtk.master-node-1 rtk.worker-node-1 rtk.worker-node-2)
IMAGES=(tsu-vps-backend:0.1.0 vpshub-frontend:0.1.0)

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

for img in "${IMAGES[@]}"; do
    file="$tmp/$(echo "$img" | tr '/:' '__').tar"
    echo ">> docker save $img"
    docker save "$img" -o "$file"
done

for node in "${NODES[@]}"; do
    for img in "${IMAGES[@]}"; do
        file="$tmp/$(echo "$img" | tr '/:' '__').tar"
        echo ">> $node <- $img"
        ssh "$node" 'sudo k3s ctr images import -' < "$file"
    done
done

echo "done"
