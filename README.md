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
			 - [X] quit: sair do programa
			 - [ ] cd: change-directory
			 - [ ] task management
				 - [X] jobs: listar tasks
				 - [ ] fg: *focar* em uma task
				 - [ ] bg: rodar em background
				 - [X] \<builtin\> &: rodar builtin em background
				 - [X] Job-Tree: Estrutura de dados para guardar as tasks
						e meta-dados
						(PIDs, JIDs, Parentescos)

 - [X] interpretação de sinais
	 - [X] SIGINT (ctrl-c): enviar para a task em *foco*
	 - [X] SIGSTP (ctrl-z): enviar para a task em *foco*

## Todo Extra

 - [ ] CLI with pwd
 - [ ] +, - identifiers in jobs
 - [ ] search commands in PATH directories

## Known Bugs

