examples = $(shell ls examples)

build:
	@for f in $(examples); do \
		$(MAKE) -C examples/$$f; \
	done
