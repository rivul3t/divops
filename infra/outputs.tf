output "vms" {
  description = "Созданные ВМ: имя → ID и IP"
  value = {
    for name, vm in decort_kvmvm.student : name => {
      id = vm.id
      ip = try(vm.network[0].ip_address, null)
    }
  }
}
