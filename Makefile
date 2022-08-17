all:
	@$(MAKE) -C librerias
	@$(MAKE) -C cliente
	@$(MAKE) -C servidor
clean:
	@$(MAKE) -C librerias clean
	@$(MAKE) -C cliente clean
	@$(MAKE) -C servidor clean
