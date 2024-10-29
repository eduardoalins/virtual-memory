## Sobre

Código desenvolvido na cadeira de Infraestrutura de Software do 3º período do curso de ciência da computação da CESAR School, sobre memórias fisicas e virtuais de um SO.

# Informações fornecidas

- A implementação deverá ser aquela em que a memória física tem apenas 128 frames.

- O programa deve ser implementado em C e ser executável em sistemas Linux, com a compilação feita por Makefile, através simplesmente do comando make via terminal, e retornar o arquivo com nome vm executável;

- Para validação da implementação, será utilizado o compilador gcc 13.2.0, com o arquivo de saída seguindo rigorosamente a formatação e texto apresentados.

- Os frames na memória física devem ser preenchido do 0 ao 127, e quando a memória estiver cheia, aplicasse o algoritmo de substituição a fim de identificar qual frame será atualizado;

- Deve-se implementar dois algoritmos de substituição de página, a saber fifo e lru, enquanto que na TLB será aplicado apenas o fifo;

- O primeiro argumento por linha de comando será um arquivo de endereços lógicos (similar ao address.txt anexado ao Classroom), o segundo argumento será o tipo de algoritmo a ser utilizado para substituição da página (fifo ou lru). Por exemplo, a chamada:

./vm address.txt lru
indica que o algoritmo de substituição da página será o lru.
