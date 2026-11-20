variable "controller_url" {
  description = "Адрес API облака (controller_url)"
  type        = string
}

variable "rg_id" {
  description = "ID ресурсной группы, в которой создаются виртуалка"
  type        = number
}

variable "storage_policy_id" {
  description = "ID политики хранения"
  type        = number
}

variable "ubuntu_image_id" {
  description = "ID убунты в облаке"
  type        = number
}

variable "net_type" {
  description = "Тип сети: EXTNET (внешняя) или VINS (внутренняя)"
  type        = string
  default     = "VINS"
}

variable "net_id" {
  description = "ID сети"
  type        = number
}

variable "students" {
  description = "Список студентов в формате имя.фамилия"
  type        = list(string)

  validation {
    condition     = alltrue([for s in var.students : can(regex("^[a-z]+\\.[a-z]+$", s))])
    error_message = "Имена должны быть в формате имя.фамилия латиницей в нижнем регистре"
  }
}

variable "ssh_public_keys" {
  description = "Публичные SSH-ключи, которые раскатятся на все ВМ"
  type        = list(string)
  default     = []
}
