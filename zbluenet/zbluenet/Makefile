MAKE = make --no-print-directory

define ECHO
	@printf "\033[;31m"; printf $1; printf "\033[0m\n"
endef

.PHONY: debug release profile clean demo

debug release profile clean:
	@$(call ECHO. "[build lib zbluenet]")
	@$(MAKE) -f mak/libzbluenet.mak $@

	@$(call ECHO. "[build demo]")
	@$(MAKE) -f mak/demo.mak $@