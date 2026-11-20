terraform {
  required_version = ">= 1.3"

  required_providers {
    decort = {
      source  = "basis/decort/decort"
      version = "4.12.4"
    }
  }
}

provider "decort" {
  authenticator  = "legacy"
  controller_url = var.controller_url
  # oauth2_url     = var.bvs_url
  # domain         = var.bvs_domain
  # allow_unverified_ssl = true
}

resource "decort_kvmvm" "student" {
  for_each = toset(var.students)

  name        = each.value #
  description = "виртуала крутого челика ${each.value}"
  rg_id       = var.rg_id

  cpu            = 2
  ram            = 4096
  boot_disk_size = 50
  image_id       = var.ubuntu_image_id
  storage_policy_id = var.storage_policy_id

  network {
    net_type = var.net_type
    net_id   = var.net_id
  }

  cloud_init = templatefile("${path.module}/cloud-init.yaml", {
    hostname = replace(each.value, ".", "-")
    ssh_keys = var.ssh_public_keys
  })
}
