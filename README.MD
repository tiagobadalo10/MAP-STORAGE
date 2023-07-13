Grupo 18

Tiago Badalo 55311
João Feliciano 52814
Miguel Sousa 48109

Não encontrámos limitações na nossa implementação.

Estratégias utilizadas na gestão de concorrência:

    table:

        - comandos put/get/del: impedir que dois clientes acessem à mesma lista da tabela ao mesmo tempo;
        - comandos size/getkeys/table_print: impedir enquanto houver clientes a fazer um put/del.
    
    stats:

        - leitura: permitir leitura quando não existem clientes a escrever ou quando existem escritores à espera (de forma a evitar starvation);
        - escrita: permitir escrita quando não existem clientes a ler ou a escrever.
