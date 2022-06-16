# Projeto final Elevador

O projeto apresenta 4 threads, todas elas são declaradas no arquivo main.c


### Serial Service: 
Essa thread tem como principal proposito receber e enviar os comandos seriais ao simulador. Alem disso ela tambem é responsavel por passar mensagens para os elevadores individualmente atraves das filas queue_d, queue_c, queue_e. A função de execucação da thread é a **thread_serial_service**.

Essa thread tambem pode ser usada para a implementação de um controle "inteligente" do elevador. Atualmente ela só repassa os comandos recebidos para a thread especifica de cada elevador, mas pode haver um preprocessamento desta informação e envia-la só para um elevador especifico.

É relevante notar que essa thread só executa quando algumas das sua flags(receber serial ou enviar serial) é setada, 

### Elevador esquerdo, central e direito :
  Existe uma thread individual para cada elevador mas todos funcionam apartir da mesma função **thread_elevador** declarada no main. O que diferencia cada elevador é a struct Elevador passada na inicialização da thread. Essas threads tem como função processar os comandos recebidos pelas filias queue_x e eventualmente enviar eventos para o simulador atraves da filha queue_out. Toda logica dos elevadores é implementada nos arquivos elevador.h e elevador.c

Atualmente o bruto do elevador esta nas funções **elevador_processa_msg** que processa a mensagem recebida na sua fila. E na função **elevador_update**.  Os andares qual o elavor tem que ir são armazenados no array **andaresPressionados** dentro da estrutura individual de cada andar. Cada andar pode ter os valores descritos na enum **StatusBotao**.

Atualmente logica atual do elevador é que ao receber um comando de subir o elevador sobe até alcançar todos andares acima dele, e funciona de maneira igual ao receber um comando descer. Ao alcançar todos os andares acima do andar original o elevador volta para o estado parado e então de acordo com os botões pressionados ele muda novamente para o estado subindo ou descendo ou continua parado.


# Informações extras

A maioria dos defines do programa são definindos no arquivo **defines.h**. E os arquivos **charbuffer.h** e **charbuffer.c** implementam um buffer circular utilizado pelo periferico do serial.

# Bugs do simulador Conhecidos

- Pro projeto funcionar você precisa do Virtual Port Simulator pq o simulador não abre a porta serial corretamente.
- Movendo na velocidade de simulação causa problemas como o elevador abrir entre andares e no pior ele literalmente sai voando pelo teto e entra pelo chão. Isso é um problema como simulador em si que não detecta corretamente os andares e não a muito que possa ser feito.