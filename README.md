# 🚀 Dragon Capsule - Sistema de Telemetria (SpaceX)

**Global Solution 2026 - Edge Computing & Computer Systems**

## 👥 Equipe (Autores)
* **Nicolas Forcione de Oliveira e Souza** - RM 566998
* **Alexandre Constantino Furtado Junior** - RM 567188
* **Matheus Freitas dos Santos** - RM 567337
* **Leonardo Batista de Souza** - RM 568558

---

## 📋 Descrição da Solução
Este projeto é um sistema de telemetria desenvolvido para a cápsula Dragon da SpaceX. O objetivo é captar parâmetros vitais da nave e transmiti-los via rádio para a Terra. O sistema atua em duas frentes: monitoramento em tempo real para a equipe em terra (via dashboard dinâmico e dados históricos) e segurança local para os tripulantes (com triggers de alertas visuais e sonoros).

## 🏗️ Arquitetura e Funcionamento

A arquitetura foi estruturada para atender aos requisitos de Edge Computing e computação em nuvem:

1. **Edge (Borda e Sensores):**
   * O cérebro local é um **ESP32** simulado no Wokwi.
   * Monitoramos **4 parâmetros** de telemetria (ultrapassando a exigência de 3):
     1. Temperatura
     2. Pressão Interna
     3. Nível de CO2
     4. Velocidade da Espaçonave
   * **Triggers e Anomalias:** Se algum parâmetro ultrapassar o limite seguro, o ESP32 atua imediatamente acionando um **LED Vermelho** (alerta visual) e um **Buzzer** (alerta sonoro) para a tripulação.

2. **Comunicação e Back-end:**
   * Os dados da telemetria são transmitidos através do protocolo **MQTT**.
   * A plataforma **FIWARE** atua como nosso back-end, recebendo as mensagens e armazenando o histórico de voo através dos seus componentes (Orion Context Broker, IoT Agent e STH-Comet).

3. **Dashboard (Equipe em Terra):**
   * Utilizando **Grafana**, construímos um painel dinâmico que consome os dados do FIWARE. A equipe pode ver a evolução da viagem em tempo de execução e analisar os gráficos de histórico.

---

## 🔗 Links Oficiais da Entrega
* 📹 **Vídeo de Apresentação (Storytelling):** [INSERIR LINK DO VÍDEO DO YOUTUBE AQUI]
* ⚙️ **Projeto no Wokwi (Circuito e Código):** [[INSERIR LINK DO WOKWI AQUI](https://wokwi.com/projects/466202907724236801)]

---

## 💻 Código e Imagens
* O código-fonte em C++ que roda no ESP32 está disponível no arquivo `main.cpp` deste repositório.
* A estrutura do circuito está no arquivo `diagram.json`.

*(Imagens do Dashboard e da Simulação)*
<img width="955" height="932" alt="Capturar" src="https://github.com/user-attachments/assets/bd6e816a-cfbb-41fd-bf14-afb433fe202a" />
<img width="871" height="856" alt="Capturar" src="https://github.com/user-attachments/assets/c82de2cf-b54f-46bb-8bfe-58f9d9395d49" />
