#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pageTableElement{
    int posicaoM;
    int pagina;
};

struct TLBelement{
    int posicaoM;
    int pagina;
};

void fifo_replacement(const char *address_file) {

    // Abre o arquivo em modo de leitura
    FILE *arquivo = fopen(address_file, "r");

    // Verifica se o arquivo foi aberto com sucesso
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.1\n");
        return;
    }
    //-------------------------------------------------------------------//
    
    int addres;// único addres
    int LINHAS = 0, l = 0;
    while (fscanf(arquivo, "%d" , &addres) == 1) {
        LINHAS++; // pegando a qtd de linhas
    }
    rewind(arquivo);
    int ADDRESSES[LINHAS];    

    while (fscanf(arquivo, "%d" , &addres) == 1) {
        ADDRESSES[l] = addres; // colocando todos os addresses em um array de int
        l++;
    }

    // Fecha o arquivo
    fclose(arquivo);
    // ---------------------------------------------------------------------
    FILE *arquivoSaida = fopen("correct.txt", "w");
    if (arquivoSaida == NULL){
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    
    
    // ---------------------------------------------------------------------
    int pageFaults = 0, TLBhit = 0;
    int memoriaIndiceEsquerdo = 0; char memoria[128][256];
    struct pageTableElement pageTable[128]; int pageTableIndice = 0, pageTableIsFree = 1;
    for (int alfa = 0; alfa < 128; alfa++){
        pageTable[alfa].pagina = -1;
    }
    struct TLBelement TLB[16]; int TLBindice = 0;
    for (int alfa = 0; alfa < 16; alfa++){
        TLB[alfa].pagina = -1;
    }

    int i = 0;
    for(i = 0 ; i < LINHAS ; i++){
        int numBin[16];// numero em binario de 16 casas
        for(int a = 0; a < 16; a++){
            numBin[a] = 0;
        }

        int numAtual = ADDRESSES[i];
        for(int t = 15; t >= 0; t--){
            if(numAtual % 2 == 0){
                numBin[t] = 0;
            }else{
                numBin[t] = 1;
            }
            numAtual /= 2;
        }

        int offset = 0, pagina = 0;

        pagina = (numBin[7]*1) + (numBin[6]*2)+ (numBin[5]*4) + (numBin[4]*8) + (numBin[3]*16) + (numBin[2]*32) + (numBin[1]*64) + (numBin[0]*128);
        offset = (numBin[15]*1) + (numBin[14]*2)+ (numBin[13]*4) + (numBin[12]*8) + (numBin[11]*16) + (numBin[10]*32) + (numBin[9]*64) + (numBin[8]*128);
        
        int pageTableAchou = 0, posicaoMemoriaAchou = -1, TLBachou = 0, posicaoTLBachou = -1;
        for (int alfa = 0; alfa < 16; alfa++){
            if ((TLB[alfa]).pagina == pagina){
                TLBachou = 1;
                posicaoMemoriaAchou = (TLB[alfa]).posicaoM;
                posicaoTLBachou = alfa;
                TLBhit++;
            }
        }
        if (TLBachou == 0){
            if (pageTableIsFree == 1){
                for (int alfa = 0; alfa < 128; alfa++){
                    if ((pageTable[alfa]).pagina == pagina){
                        pageTableAchou = 1;

                        posicaoMemoriaAchou = (pageTable[alfa]).posicaoM;

                        TLB[TLBindice].pagina = pagina;
                        TLB[TLBindice].posicaoM = posicaoMemoriaAchou;
                        posicaoTLBachou = TLBindice;
                        TLBindice++;
                        if (TLBindice == 16){
                            TLBindice = 0;
                        }
                    }
                }
                if (pageTableAchou == 0){
                    (pageTable[pageTableIndice]).pagina = pagina;
                    (pageTable[pageTableIndice]).posicaoM = pageTableIndice;
                    posicaoMemoriaAchou = pageTableIndice;
                    pageTableIndice++;
                    if (pageTableIndice == 128){
                        pageTableIndice = 0;
                        pageTableIsFree = 0;
                    }
                    pageTableAchou = 1;
                    
                    //----------------------------------------------------------
                    FILE *arquivoBin = fopen("BACKING_STORE.bin", "rb");
                    if (arquivoBin == NULL) {
                        perror("Erro ao abrir o arquivo BACKING_STORE.bin");
                        return; 
                    }

                    char buffer[256];

                    if (pagina < 0) {
                        fprintf(stderr, "Valor inválido para 'pagina': %d\n", pagina);
                        fclose(arquivoBin);
                        return; 
                    }

                    if (fseek(arquivoBin, pagina*256, SEEK_SET) != 0) {
                        perror("Erro ao posicionar o ponteiro do arquivo");
                        fclose(arquivoBin);
                        return; 
                    }

                    size_t bytesRead = fread(buffer, sizeof(char), 256, arquivoBin);
                    if (bytesRead != 256) {
                        if (feof(arquivoBin)) {
                            fprintf(stderr, "Fim do arquivo atingido prematuramente.\n");
                        } else if (ferror(arquivoBin)) {
                            perror("Erro ao ler do arquivo BACKING_STORE.bin");
                        }
                        fclose(arquivoBin);
                        return; 
                    }

                    for (int gama = 0; gama < 256; gama++) {
                        memoria[memoriaIndiceEsquerdo][gama] = buffer[gama];
                    }

                    fclose(arquivoBin);
                    
                    TLB[TLBindice].pagina = pagina;
                    TLB[TLBindice].posicaoM = memoriaIndiceEsquerdo;
                    posicaoTLBachou = TLBindice;
                    TLBindice++;
                    if (TLBindice == 16){
                        TLBindice = 0;
                    }

                    memoriaIndiceEsquerdo++;
                    if (memoriaIndiceEsquerdo == 128){
                        memoriaIndiceEsquerdo = 0;
                    }

                    pageFaults++;

                    //----------------------------------------------------------
                }
            }else{
                for (int alfa = 0; alfa < 128; alfa++){
                    if ((pageTable[alfa]).pagina == pagina){
                        pageTableAchou = 1;
                        posicaoMemoriaAchou = (pageTable[alfa]).posicaoM;

                        TLB[TLBindice].pagina = pagina;
                        TLB[TLBindice].posicaoM = posicaoMemoriaAchou;
                        posicaoTLBachou = TLBindice;
                        TLBindice++;
                        if (TLBindice == 16){
                            TLBindice = 0;
                        }
                    }   
                }
            }
            if (pageTableAchou == 1){
                //printf("%d %d %d\n", memoria[posicaoMemoriaAchou][offset], ADDRESSES[i], ((posicaoMemoriaAchou*256)+offset));
                
            }else{

                //----------------------------------------------------------
                FILE *arquivoBin = fopen("BACKING_STORE.bin", "rb");
                if (arquivoBin == NULL) {
                    perror("Erro ao abrir o arquivo BACKING_STORE.bin");
                    return; 
                }

                char buffer2[256];

                if (pagina < 0) {
                    fprintf(stderr, "Valor inválido para 'pagina': %d\n", pagina);
                    fclose(arquivoBin);
                    return; 
                }

                if (fseek(arquivoBin, pagina*256, SEEK_SET) != 0) {
                    perror("Erro ao posicionar o ponteiro do arquivo");
                    fclose(arquivoBin);
                    return; 
                }

                size_t bytesRead2 = fread(buffer2, sizeof(char), 256, arquivoBin);
                if (bytesRead2 != 256) {
                    if (feof(arquivoBin)) {
                        fprintf(stderr, "Fim do arquivo atingido prematuramente.\n");
                    } else if (ferror(arquivoBin)) {
                        perror("Erro ao ler do arquivo BACKING_STORE.bin");
                    }
                    fclose(arquivoBin);
                    return; 
                }

                for (int gama = 0; gama < 256; gama++) {
                    memoria[memoriaIndiceEsquerdo][gama] = buffer2[gama];
                }

                fclose(arquivoBin);

                TLB[TLBindice].pagina = pagina;
                TLB[TLBindice].posicaoM = memoriaIndiceEsquerdo;
                posicaoTLBachou = TLBindice;
                TLBindice++;
                if (TLBindice == 16){
                    TLBindice = 0;
                }

                memoriaIndiceEsquerdo++;
                if (memoriaIndiceEsquerdo == 128){
                    memoriaIndiceEsquerdo = 0;
                }
                //----------------------------------------------------------

                (pageTable[pageTableIndice]).pagina = pagina;
                (pageTable[pageTableIndice]).posicaoM = pageTableIndice;
                posicaoMemoriaAchou = pageTableIndice;
                pageTableIndice++;
                if (pageTableIndice == 128){
                    pageTableIndice = 0;
                }

                pageFaults++;

                //printf("%d %d %d\n", memoria[posicaoMemoriaAchou][offset], ADDRESSES[i], ((posicaoMemoriaAchou*256)+offset));
            }
        }
        //printf("Virtual address: %d TLB: %d Physical address: %d Value: %d\n", ADDRESSES[i], posicaoTLBachou, ((posicaoMemoriaAchou*256)+offset), memoria[posicaoMemoriaAchou][offset]);
        fprintf(arquivoSaida, "Virtual address: %d TLB: %d Physical address: %d Value: %d\n", ADDRESSES[i], posicaoTLBachou, ((posicaoMemoriaAchou*256)+offset), memoria[posicaoMemoriaAchou][offset]);

    }
    fprintf(arquivoSaida, "Number of Translated Addresses = %d\n", LINHAS);
    fprintf(arquivoSaida, "Page Faults = %d\n", pageFaults);
    float pageFaultsRate = ((float)pageFaults)/((float)LINHAS);
    fprintf(arquivoSaida, "Page Fault Rate = %.3f\n", (pageFaultsRate));
    fprintf(arquivoSaida, "TLB Hits = %d\n", TLBhit);
    float TLBhitRate = ((float)TLBhit)/((float)LINHAS);
    fprintf(arquivoSaida, "TLB Hit Rate = %.3f\n", (TLBhitRate));
    fclose(arquivoSaida);
}

void lru_replacement(const char *address_file) {

    // Abre o arquivo em modo de leitura
    FILE *arquivo = fopen(address_file, "r");

    // Verifica se o arquivo foi aberto com sucesso
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }
    //-------------------------------------------------------------------//
    
    int addres;// único addres
    int LINHAS = 0, l = 0;
    while (fscanf(arquivo, "%d" , &addres) == 1) {
        LINHAS++; // pegando a qtd de linhas
    }
    rewind(arquivo);
    int ADDRESSES[LINHAS];    

    while (fscanf(arquivo, "%d" , &addres) == 1) {
        ADDRESSES[l] = addres; // colocando todos os addresses em um array de int
        l++;
    }

    // Fecha o arquivo
    fclose(arquivo);
    // ---------------------------------------------------------------------
    FILE *arquivoSaida = fopen("correct.txt", "w");
    if (arquivoSaida == NULL){
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    // ---------------------------------------------------------------------
    int pageFaults = 0, TLBhit = 0;
    int memoriaIndiceEsquerdo = 0; char memoria[128][256]; int posicaoMemoriaIndice = 0;
    struct pageTableElement pageTable[128], pageTableBackup[128];int pageTableIsFree = 1;
    for (int alfa = 0; alfa < 128; alfa++){
        pageTable[alfa].pagina = -1;
    }
    struct TLBelement TLB[16]; int TLBindice = 0;
    for (int alfa = 0; alfa < 16; alfa++){
        TLB[alfa].pagina = -1;
    }

    int i = 0;
    for(i = 0 ; i < LINHAS ; i++){ //****************INICIO***********************
        int numBin[16];// numero em binario de 16 casas
        for(int a = 0; a < 16; a++){
            numBin[a] = 0;
        }

        int numAtual = ADDRESSES[i];
        for(int t = 15; t >= 0; t--){
            if(numAtual % 2 == 0){
                numBin[t] = 0;
            }else{
                numBin[t] = 1;
            }
            numAtual /= 2;
        }

        int offset = 0, pagina = 0;

        pagina = (numBin[7]*1) + (numBin[6]*2)+ (numBin[5]*4) + (numBin[4]*8) + (numBin[3]*16) + (numBin[2]*32) + (numBin[1]*64) + (numBin[0]*128);
        offset = (numBin[15]*1) + (numBin[14]*2)+ (numBin[13]*4) + (numBin[12]*8) + (numBin[11]*16) + (numBin[10]*32) + (numBin[9]*64) + (numBin[8]*128);
        
        int pageTableAchou = 0, posicaoMemoriaAchou = -1, TLBachou = 0, posicaoTLBachou = -1;
        for (int alfa = 0; alfa < 16; alfa++){
            if ((TLB[alfa]).pagina == pagina){
                TLBachou = 1;
                posicaoMemoriaAchou = (TLB[alfa]).posicaoM;
                posicaoTLBachou = alfa;
                TLBhit++;

                int posicaoPageTable;
                for (int beta = 0; beta < 128; beta++){
                    if (pageTable[beta].pagina == pagina){
                        posicaoPageTable = beta;
                    }
                    pageTableBackup[beta].pagina = pageTable[beta].pagina;
                    pageTableBackup[beta].posicaoM = pageTable[beta].posicaoM;
                }
                
                pageTable[0].pagina = pagina;
                pageTable[0].posicaoM = TLB[alfa].posicaoM;

                for (int beta = 1; beta <= posicaoPageTable; beta++){
                    pageTable[beta] = pageTableBackup[beta-1];
                }
                
            }
        }
        if (TLBachou == 0){
            if (pageTableIsFree == 1){
                for (int alfa = 0; alfa < 128; alfa++){
                    if ((pageTable[alfa]).pagina == pagina){
                        pageTableAchou = 1;

                        posicaoMemoriaAchou = (pageTable[alfa]).posicaoM;

                        TLB[TLBindice].pagina = pagina;
                        TLB[TLBindice].posicaoM = posicaoMemoriaAchou;
                        posicaoTLBachou = TLBindice;
                        TLBindice++;
                        if (TLBindice == 16){
                            TLBindice = 0;
                        }

                        int posicaoPageTable2;
                        for (int beta = 0; beta < 128; beta++){
                            if (pageTable[beta].pagina == pagina){
                                posicaoPageTable2 = beta;
                            }
                            pageTableBackup[beta].pagina = pageTable[beta].pagina;
                            pageTableBackup[beta].posicaoM = pageTable[beta].posicaoM;
                        }
                        
                        pageTable[0].pagina = pagina;
                        pageTable[0].posicaoM = posicaoMemoriaAchou;

                        for (int beta = 1; beta <= posicaoPageTable2; beta++){
                            pageTable[beta] = pageTableBackup[beta-1];
                        }

                    }
                }
                if (pageTableAchou == 0){
                    for (int beta = 0; beta < 128; beta++){
                        pageTableBackup[beta].pagina = pageTable[beta].pagina;
                        pageTableBackup[beta].posicaoM = pageTable[beta].posicaoM;
                    }

                    (pageTable[0]).pagina = pagina;
                    (pageTable[0]).posicaoM = posicaoMemoriaIndice;

                    for (int beta = 1; beta < 128; beta++){
                        pageTable[beta] = pageTableBackup[beta-1];
                    }

                    posicaoMemoriaAchou = posicaoMemoriaIndice;
                    posicaoMemoriaIndice++;
                    if (posicaoMemoriaIndice == 128){
                        posicaoMemoriaIndice = 0;
                        pageTableIsFree = 0;
                    }
                    pageTableAchou = 1;
                    
                    //----------------------------------------------------------
                    FILE *arquivoBin = fopen("BACKING_STORE.bin", "rb");
                    if (arquivoBin == NULL) {
                        perror("Erro ao abrir o arquivo BACKING_STORE.bin");
                        return; 
                    }

                    char buffer[256];

                    if (pagina < 0) {
                        fprintf(stderr, "Valor inválido para 'pagina': %d\n", pagina);
                        fclose(arquivoBin);
                        return; 
                    }

                    if (fseek(arquivoBin, pagina*256, SEEK_SET) != 0) {
                        perror("Erro ao posicionar o ponteiro do arquivo");
                        fclose(arquivoBin);
                        return; 
                    }

                    size_t bytesRead = fread(buffer, sizeof(char), 256, arquivoBin);
                    if (bytesRead != 256) {
                        if (feof(arquivoBin)) {
                            fprintf(stderr, "Fim do arquivo atingido prematuramente.\n");
                        } else if (ferror(arquivoBin)) {
                            perror("Erro ao ler do arquivo BACKING_STORE.bin");
                        }
                        fclose(arquivoBin);
                        return; 
                    }

                    for (int gama = 0; gama < 256; gama++) {
                        memoria[memoriaIndiceEsquerdo][gama] = buffer[gama];
                    }

                    fclose(arquivoBin);
                    
                    TLB[TLBindice].pagina = pagina;
                    TLB[TLBindice].posicaoM = memoriaIndiceEsquerdo;
                    posicaoTLBachou = TLBindice;
                    TLBindice++;
                    if (TLBindice == 16){
                        TLBindice = 0;
                    }

                    memoriaIndiceEsquerdo++;
                    if (memoriaIndiceEsquerdo == 128){
                        memoriaIndiceEsquerdo = 0;
                    }

                    pageFaults++;

                    //----------------------------------------------------------
                }
            }else{
                for (int alfa = 0; alfa < 128; alfa++){
                    if ((pageTable[alfa]).pagina == pagina){
                        pageTableAchou = 1;
                        posicaoMemoriaAchou = (pageTable[alfa]).posicaoM;

                        TLB[TLBindice].pagina = pagina;
                        TLB[TLBindice].posicaoM = posicaoMemoriaAchou;
                        posicaoTLBachou = TLBindice;
                        TLBindice++;
                        if (TLBindice == 16){
                            TLBindice = 0;
                        }

                        int posicaoPageTable3;
                        for (int beta = 0; beta < 128; beta++){
                            if (pageTable[beta].pagina == pagina){
                                posicaoPageTable3 = beta;
                            }
                            pageTableBackup[beta].pagina = pageTable[beta].pagina;
                            pageTableBackup[beta].posicaoM = pageTable[beta].posicaoM;
                        }
                        
                        pageTable[0].pagina = pagina;
                        pageTable[0].posicaoM = posicaoMemoriaAchou;

                        for (int beta = 1; beta <= posicaoPageTable3; beta++){
                            pageTable[beta] = pageTableBackup[beta-1];
                        }

                    }   
                }
            }
            if (pageTableAchou == 1){
                //printf("%d %d %d\n", memoria[posicaoMemoriaAchou][offset], ADDRESSES[i], ((posicaoMemoriaAchou*256)+offset));
                
            }else{

                //----------------------------------------------------------
                FILE *arquivoBin = fopen("BACKING_STORE.bin", "rb");
                if (arquivoBin == NULL) {
                    perror("Erro ao abrir o arquivo BACKING_STORE.bin");
                    return; 
                }

                char buffer2[256];

                if (pagina < 0) {
                    fprintf(stderr, "Valor inválido para 'pagina': %d\n", pagina);
                    fclose(arquivoBin);
                    return; 
                }

                if (fseek(arquivoBin, pagina*256, SEEK_SET) != 0) {
                    perror("Erro ao posicionar o ponteiro do arquivo");
                    fclose(arquivoBin);
                    return; 
                }

                size_t bytesRead2 = fread(buffer2, sizeof(char), 256, arquivoBin);
                if (bytesRead2 != 256) {
                    if (feof(arquivoBin)) {
                        fprintf(stderr, "Fim do arquivo atingido prematuramente.\n");
                    } else if (ferror(arquivoBin)) {
                        perror("Erro ao ler do arquivo BACKING_STORE.bin");
                    }
                    fclose(arquivoBin);
                    return; 
                }
                
                int enderecoDisponivel = pageTable[127].posicaoM;

                for (int gama = 0; gama < 256; gama++) {
                    memoria[enderecoDisponivel][gama] = buffer2[gama];
                }

                fclose(arquivoBin);

                TLB[TLBindice].pagina = pagina;
                TLB[TLBindice].posicaoM = enderecoDisponivel;
                posicaoTLBachou = TLBindice;
                TLBindice++;
                if (TLBindice == 16){
                    TLBindice = 0;
                }
                
                for (int beta = 0; beta < 128; beta++){
                    pageTableBackup[beta].pagina = pageTable[beta].pagina;
                    pageTableBackup[beta].posicaoM = pageTable[beta].posicaoM;
                }

                (pageTable[0]).pagina = pagina;
                (pageTable[0]).posicaoM = enderecoDisponivel;

                for (int beta = 1; beta < 128; beta++){
                    pageTable[beta] = pageTableBackup[beta-1];
                }

                pageFaults++;

                //printf("%d %d %d\n", memoria[posicaoMemoriaAchou][offset], ADDRESSES[i], ((posicaoMemoriaAchou*256)+offset));
            }
        }
        //printf("Virtual address: %d TLB: %d Physical address: %d Value: %d\n", ADDRESSES[i], posicaoTLBachou, ((posicaoMemoriaAchou*256)+offset), memoria[posicaoMemoriaAchou][offset]);
        int printPos;
        for (int c = 0; c < 128; c++){
            if (pageTable[c].pagina == pagina){
                printPos = pageTable[c].posicaoM;
            }
        }
        fprintf(arquivoSaida, "Virtual address: %d TLB: %d Physical address: %d Value: %d\n", ADDRESSES[i], posicaoTLBachou, ((printPos*256)+offset), memoria[printPos][offset]);
    
    }
    fprintf(arquivoSaida, "Number of Translated Addresses = %d\n", LINHAS);
    fprintf(arquivoSaida, "Page Faults = %d\n", pageFaults);
    float pageFaultsRate = ((float)pageFaults)/((float)LINHAS);
    fprintf(arquivoSaida, "Page Fault Rate = %.3f\n", (pageFaultsRate));
    fprintf(arquivoSaida, "TLB Hits = %d\n", TLBhit);
    float TLBhitRate = ((float)TLBhit)/((float)LINHAS);
    fprintf(arquivoSaida, "TLB Hit Rate = %.3f\n", (TLBhitRate));
    fclose(arquivoSaida);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo de endereços> <algoritmo (fifo|lru)>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *address_file = argv[1];
    const char *algorithm = argv[2];

    if (strcmp(algorithm, "fifo") == 0) {
        fifo_replacement(address_file);
    } else if (strcmp(algorithm, "lru") == 0) {
        lru_replacement(address_file);
    } else {
        fprintf(stderr, "Algoritmo desconhecido: %s. Use 'fifo' ou 'lru'.\n", algorithm);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}