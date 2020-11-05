# shell-prototype

O objetivo é fazer uma shell com funcionalidades básicas.

## Funcionalidades básicas

[ ] interação
	[ ] input parser
[ ] task management
	[ ] jobs: listar tasks
	[ ] fg: "focar" em uma task
	[ ] bg: rodar em background
	[ ] <task> &: rodar task em background
	[ ] Job-Tree: Estrutura de dados para guardar as tasks e meta-dados
		(PIDs, JIDs, Parentescos)
[ ] built-in
	[ ] quit: sair do programa
	[ ] cd: change-directory
	[ ] ./<bin>: executar binários
[ ] interpretação de sinais: SIGINT (ctrl-c), SIGSTP (ctrl-z), EOF (ctrl-d)
