# Braço Evolutivo

Um braço mecânico evolutivo multiobjetivo. O projeto foi feito utilizando a biblioteca SDL2 para a parte gráfica em conjunto com bibliotecas padrão da linguagem C.
O projeto segue conceitos ensinados na disciplina de Sistemas Evolutivos aplicados à robótica.

A ideia consiste num braço cujo comprimento é determinado em um define pelo usuário, com o número de juntas também definido pelo usuário no início do código,
que deve chegar próximo a um ponto objetivo, definido pelo usuário. A aplicação é multiobjetiva, já que após o indivíduo chegar numa certa distância do objetivo,
também se considera a sua menor distância a algum obstáculo como parte da pontuação(A descrição da avaliação no código é mais precisa).

# Instalando SDL2

Uma análise mais completa de como instalar a bilbioteca pode ser encontrada em https://wiki.libsdl.org/SDL2/Installation .

Obs:  Se quiser rodar no linux com VS Code, encontre as configuraçõescom o atalho CTRL + SHIFT + P, entre em C/C++ Edit Configuration(JSON). Dentro do include path, inclua "/usr/include/SDL2"

Mas, em resumo, é possível apenas executar os comandos em ubuntu:


sudo su  //(para se tornar o usuário root)

apt-get update // para receber todos os pacotes das versões mais recentes

apt-get upgrade // para atualizar todos os pacotes

apt-get install libsdl2-dev


exit //(para sair do usuario root)

Isso deveria ser o suficiente, já que como usuário root, a biblioteca deve ser instalada no local padrão de bibliotecas. Contudo, 
parece ser um problema comum que mesmo assim a biblioteca acabe parando em outro lugar. Para tentar contornar isso,
compile o código com:

gcc braco_mecanico.c -o braco $(sdl2-config --cflags --libs) -lSDL2 -lm

# Como usar o programa

São 3 códigos, o desenha.c, braco_mecanicoav.c e o a_starav.c. Todos essses possuem comentarios com a linha de comando para compilação.

Para desenhar e salvar um cenário, compile o desenha.c e chame o executável no terminal com o nome do arquivo a ser salvo.
Exemplo: ./desenha cenarioInteressante, o programa será executado e podera desenhar com cliques do mouse um cenário, aperte D para marcar o ponto de destino e O para o ponto de origem, quando terminar basta fechar o programa.
Dessa maneira, um arquivo binário será gerado com esse nome e conterá uma matriz de inteiros representando o cenário que gerou. O arquivo somente será salvo se houver um destino e uma origem desenhados.

Agora, basta compilar o programa de a* e executá-lo com o mesmo nome do cenario desejado.
Exemplo: ./a_star cenarioInteressante
Dessa maneira, o a_star irá ler o cenário salvo e gerar um caminho mínimo que será salvo em um novo arquivo de nome caminho_cenarioInteressante

Finalmente, o braço. Após ser compilado, basta que se execute ele por ./braco cenarioInteressante caminho_cenarioInteressante NUM_JUNTAS COMPRIMENTO_TOTAL NUM_INDIVIDUOS

# Forma de Uso Alternativa - Interface

Simultâneamente, uma interface gráfica tem sido desenvolvida pelo grupo de iniciação científica, melhor detalhada no repositório: https://github.com/nicocarreiro/Multi-use-Interface que facilita o proccesso de criar cenários, configurar experimentos e modificar estratégias. Para utiliza-la basta ter os códigos compilados com o main.py em uma pastar e executar python3 main.py

# Demonstração de Uso



## Versões Anteriores / Desenvolvimento

Exemplo sem obstáculos, o braço acha uma configuração quase instantaneamente.

https://github.com/LVinaud/bracoEvolutivo/assets/128495824/df3cf7a9-29bb-47ed-aab2-2b2755683bf2



Teste da tecla Q, reseta a população. Novamente, em pouco tempo uma nova configuração é encontrada.

https://github.com/LVinaud/bracoEvolutivo/assets/128495824/daa3b244-391c-4989-9d47-1bbd62c87486



Teste com obstáculos, chegar ao objetivo é muito rápido. Após um certo tempo o braço aprende a se distanciar dos obstáculos.

https://github.com/LVinaud/bracoEvolutivo/assets/128495824/dd4a6ecf-5448-4a9f-9b55-e6c6c30fb351


Exemplo de execução do programa mostrando toda a população.

https://github.com/LVinaud/bracoEvolutivo/assets/128495824/37e0ec29-7f43-4ccf-aba3-525bddbc23be





