# Braço Evolutivo

Um braço mecânico evolutivo multiobjetivo. O projeto foi feito utilizando a biblioteca SDL2 para a parte gráfica em conjunto com bibliotecas padrão da linguagem C.
O projeto segue conceitos ensinados na disciplina de Sistemas Evolutivos aplicados à robótica.

A ideia consiste num braço cujo comprimento é determinado em um define pelo usuário, com um números de juntas também definido pelo usuário no início do código.
que deve chegar próximo a um ponto objetivo, definido pelo usuário. A aplicação é multiobjetiva, já que após o indivíduo chegar numa distância inferior à um valor mínimo,
também se considera a sua menor distância a algum obstaculos como parte da pontuação(A descrição da avaliação no código é mais precisa).

# Como usar o programa

Após ajustar os defines da maneira como o usuário preferir, incluindo o delay de tempo a cada iteração do código, o número de individuos,
número de juntas, comprimento do braço, a distância mínima para que se inicie a segunda parte da avaliação de um indivíduo,
quantidade de gerações repetidas para uma nova mutação e a escolha entre mostrar apenas
o melhor individuo histórico ou toda a população; rode o programa.

Primeiramente ele espera que o usuario defina os obstaculos, quando terminar de desenhá-los(um obstaculo é definido por dois cliques que formarão um retângulo), aperte enter. 
Agora, basta clicar onde os braços partirão e o programa se inicia. A cada clique do mouse de agora em diante, o objetivo será redefinido para a nova posição. 
A tecla Q gera um genocídio na população, enquanto a Z aumenta a mutação e X diminui. Caso nao ajuste a mutação manualmente, ela será reajustada automaticamente seguindo 
o conceito de mutação variável.


Exemplo sem obstáculos, o braço acha uma configuração quase instantâneamente.

https://github.com/LVinaud/bracoEvolutivo/assets/128495824/df3cf7a9-29bb-47ed-aab2-2b2755683bf2

Teste da tecla Q, reseta a população. Novamente, em pouco tempo uma nova configuração é encontrada.

https://github.com/LVinaud/bracoEvolutivo/assets/128495824/daa3b244-391c-4989-9d47-1bbd62c87486

Teste com obstáculos, chegar ao objetivo é muito rápido. Após um certo tempo o braço aprende a se distanciar dos obstáculos.

https://github.com/LVinaud/bracoEvolutivo/assets/128495824/dd4a6ecf-5448-4a9f-9b55-e6c6c30fb351




