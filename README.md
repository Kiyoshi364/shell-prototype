# shell-prototype

O objetivo é fazer uma shell com funcionalidades básicas.

## Funcionalidades básicas

 - [ ] interação
	 - [ ] input parser
	 - [ ] command runner
		- [ ] built-in
			 - [ ] quit: sair do programa
			 - [ ] cd: change-directory
			 - [ ] \<PATH\>: executar binários
		- [ ] task management
			 - [ ] jobs: listar tasks
			 - [ ] fg: *focar* em uma task
			 - [ ] bg: rodar em background
			 - [ ] <task> &: rodar task em background
			 - [ ] Job-Tree: Estrutura de dados para guardar as tasks
					e meta-dados
					(PIDs, JIDs, Parentescos)

- [ ] interpretação de sinais
	- [ ] SIGINT (ctrl-c): enviar para a task em *foco*
	- [ ] SIGSTP (ctrl-z): enviar para a task em *foco*
	- [ ] EOF (ctrl-d): enviar para a task em *foco*
