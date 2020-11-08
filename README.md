# shell-prototype

O objetivo é fazer uma shell com funcionalidades básicas.

## Funcionalidades básicas

 - [X] interação
	 - [X] EOF (ctrl-d): Terminar ao ler EOF
	 - [X] input parser
	 - [X] command runner
		- [X] general commands
			 - [X] \<task\> &: rodar task em background
		- [X] built-in
			 - [X] quit: sair do programa
			 - [X] cd: change-directory
			 - [X] task management
				 - [X] jobs: listar tasks
				 - [X] fg: *focar* em uma task
				 - [X] bg: rodar em background
				 - [X] \<builtin\> &: rodar builtin em background
				 - [X] Job-Tree: Estrutura de dados para guardar as tasks
						e meta-dados
						(PIDs, JIDs, Parentescos)

 - [X] interpretação de sinais
	 - [X] SIGINT (ctrl-c): enviar para a task em *foco*
	 - [X] SIGSTP (ctrl-z): enviar para a task em *foco*

## Todo Extra

 - [X] CLI with pwd
 - [ ] +, - identifiers in jobs
 - [ ] search commands in PATH directories

## Known Bugs

 - Signaling with SIGSTOP, may cause task manager to lose track of task.
