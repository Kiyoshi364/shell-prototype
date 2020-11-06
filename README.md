# shell-prototype

O objetivo é fazer uma shell com funcionalidades básicas.

## Funcionalidades básicas

 - [ ] interação
	 - [X] input parser
	 - [ ] command runner
		- [X] general commands
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
	 - [ ] EOF (ctrl-d): Terminar ao ler EOF

 - [ ] interpretação de sinais
	 - [ ] SIGINT (ctrl-c): enviar para a task em *foco*
	 - [ ] SIGSTP (ctrl-z): enviar para a task em *foco*
