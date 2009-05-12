LINUX = ./linux-2.6

version = $(KVM_VERSION)

_hack = mv $1 $1.orig && \
	gawk -v version=$(version) -f $(ARCH_DIR)/hack-module.awk $1.orig \
	    | sed '/\#include/! s/\blapic\b/l_apic/g' > $1 && rm $1.orig

unifdef = mv $1 $1.orig && cat unifdef.h $1.orig > $1 && rm $1.orig

hack = $(call _hack,$T/$(strip $1))

hack-files-x86 = kvm_main.c mmu.c vmx.c svm.c x86.c irq.h lapic.c i8254.c kvm_trace.c timer.c
hack-files-ia64 = kvm_main.c kvm_fw.c kvm_lib.c kvm-ia64.c

hack-files = $(hack-files-$(ARCH_DIR))

sync: header-sync source-sync

T = $(subst -sync,,$@)-tmp

headers-old = $(LINUX)/./include/asm-$(ARCH_DIR)/kvm*.h
headers-new = $(LINUX)/arch/$(ARCH_DIR)/include/asm/./kvm*.h \
	$(LINUX)/arch/$(ARCH_DIR)/include/asm/./vmx*.h \
	$(LINUX)/arch/$(ARCH_DIR)/include/asm/./svm*.h \
	$(LINUX)/arch/$(ARCH_DIR)/include/asm/./virtext*.h

header-sync:
	rm -rf $T
	rsync -R -L \
	     "$(LINUX)"/./include/linux/kvm*.h \
	     $(if $(wildcard $(headers-old)), $(headers-old)) \
             $T/
	$(if $(wildcard $(headers-new)), \
	rsync -R -L \
	     $(wildcard $(headers-new)) \
             $T/include/asm-$(ARCH_DIR)/)

	for i in $$(find $T -name '*.h'); do \
		$(call unifdef,$$i); done
	$(call hack, include/linux/kvm.h)
	$(call hack, include/asm-$(ARCH_DIR)/kvm.h)
	set -e && for i in $$(find $T -type f -printf '%P '); \
		do mkdir -p $$(dirname $$i); cmp -s $$i $T/$$i || cp $T/$$i $$i; done
	rm -rf $T

source-sync:
	rm -rf $T
	rsync --exclude='*.mod.c' -R \
	     "$(LINUX)"/arch/$(ARCH_DIR)/kvm/./*.[cSh] \
	     "$(LINUX)"/virt/kvm/./*.[cSh] \
	     $T/

	for i in $$(find $T -name '*.c'); do \
		$(call unifdef,$$i); done

	for i in $(hack-files); \
		do $(call hack, $$i); done

	for i in $$(find $T -type f -printf '%P '); \
		do cmp -s $(ARCH_DIR)/$$i $T/$$i || cp $T/$$i $(ARCH_DIR)/$$i; done
	rm -rf $T

