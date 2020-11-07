# shell-prototype

O objetivo é fazer uma shell com funcionalidades básicas.

## Funcionalidades básicas

 - [ ] interação
	 - [X] EOF (ctrl-d): Terminar ao ler EOF
	 - [X] input parser
	 - [ ] command runner
		- [X] general commands
			 - [X] \<task\> &: rodar task em background
		- [ ] built-in
			 - [ ] quit: sair do programa
			 - [ ] cd: change-directory
			 - [ ] task management
				 - [ ] jobs: listar tasks
				 - [ ] fg: *focar* em uma task
				 - [ ] bg: rodar em background
				 - [ ] \<builtin\> &: rodar builtin em background
				 - [X] Job-Tree: Estrutura de dados para guardar as tasks
						e meta-dados
						(PIDs, JIDs, Parentescos)

 - [ ] interpretação de sinais
	 - [ ] SIGINT (ctrl-c): enviar para a task em *foco*
	 - [ ] SIGSTP (ctrl-z): enviar para a task em *foco*
