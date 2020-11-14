# Comparação entre componentes

## ADT7641A
É feito para ser utilizado em conjunto com um diodo termosensível. Logo, foge do propósito.

## EMC1412
Também utiliza um diodo externo mas, possui um diodo interno e consegue monitorar a temperatura dos dois.

## LM35
Saída do sensor é analógica, o que é uma boa opção para o nosso propósito, mas, o nível de tensão de alimentação é acima de 3.3 V o que é um limitante visto que a tensão de trabalho do nosso microcontrolador é 3.3 V.

## TMP35
Saída de temperatura analógica, o que pode ser uma boa opção visto o menor uso da cpu para o processo de aquisição de dados.
A princípio achei as definições do datasheet um pouco confusas e, caso seja cogitado o uso deste CI teria que ser revisada a parte de influência de sinais RF próximos e a necessidade de off-set de tensão.

## TMP235 
Parece uma boa opção tendo em conta a facilidade de implementação e o baixo custo de processamento. Possui preço relativametne baixo. Aparenta ser mais seguro que o TMP35 e mais resistente a ruídos. O datasheet possui design de referência.
