#include <bits/stdc++.h>
using namespace std;

// Символы ленты
enum Sym { H_SYM = 0, Z0 = 1, Z1 = 2, STAR = 3, A = 4, B = 5 };

// Состояния машины
enum State { q0 = 0, q0c = 1, q1 = 2, q2a = 3, q2b = 4, q3 = 5, q5 = 6, qH = 7 };

struct Command { int new_state; int write_sym; int move; };

// движения
const int L = -1, H = 0, R = 1;

// Таблица переходов: states x symbols
Command T[20][10];

int sym_to_id(char c) {
    if (c=='h') return H_SYM;
    if (c=='0') return Z0;
    if (c=='1') return Z1;
    if (c=='*') return STAR;
    if (c=='A') return A;
    if (c=='B') return B;
    return H_SYM;
}

char id_to_sym(int x) {
    switch(x) {
        case H_SYM: return 'h';
        case Z0: return '0';
        case Z1: return '1';
        case STAR: return '*';
        case A: return 'A';
        case B: return 'B';
        default: return 'h';
    }
}

void setCmd(int s, int sym, int ns, int ws, int mv) {
    T[s][sym] = {ns, ws, mv};
}

void init_table() {
    // Инициализируем все команды как отсутствие перехода (new_state = -1)
    for(int i=0;i<20;i++) for(int j=0;j<10;j++) T[i][j] = {-1, H_SYM, 0};

    // q0: пропуск начальных h, при встрече первого бита переход в q0c (начать комплементацию)
    setCmd(q0, H_SYM, q0, H_SYM, R);
    setCmd(q0, Z0, q0c, Z1, R);
    setCmd(q0, Z1, q0c, Z0, R);

    // q0c: комплементация всех битов до правого h; при встрече h -> записать '*' и перейти в q1
    setCmd(q0c, Z0, q0c, Z1, R);
    setCmd(q0c, Z1, q0c, Z0, R);
    setCmd(q0c, H_SYM, q1, STAR, L);
    // защитные переходы
    setCmd(q0c, STAR, q0c, STAR, R);

    // q1: найти справа-лево самый правый немаркированный бит (идем влево от '*')
    setCmd(q1, A, q1, A, L);
    setCmd(q1, B, q1, B, L);
    setCmd(q1, Z1, q2a, A, R); // Z1 -> пометить A (означает '1' в копии), идти вправо записывать
    setCmd(q1, Z0, q2b, B, R); // Z0 -> пометить B (означает '0' в копии), идти вправо записывать
    setCmd(q1, H_SYM, q5, H_SYM, R); // если дошли до левого h — все биты помечены, переход к очистке

    // q2a: идём вправо до первого h, затем записываем '1' и возвращаемся в q3
    setCmd(q2a, Z0, q2a, Z0, R);
    setCmd(q2a, Z1, q2a, Z1, R);
    setCmd(q2a, A, q2a, A, R);
    setCmd(q2a, B, q2a, B, R);
    setCmd(q2a, STAR, q2a, STAR, R);
    setCmd(q2a, H_SYM, q3, Z1, L); // записать 1 в позицию правого h (копия)

    // q2b: аналогично для '0'
    setCmd(q2b, Z0, q2b, Z0, R);
    setCmd(q2b, Z1, q2b, Z1, R);
    setCmd(q2b, A, q2b, A, R);
    setCmd(q2b, B, q2b, B, R);
    setCmd(q2b, STAR, q2b, STAR, R);
    setCmd(q2b, H_SYM, q3, Z0, L); // записать 0

    // q3: возвращаемся влево до только что помеченного символа (A или B), затем сдвигаемся влево и снова в q1
    setCmd(q3, Z0, q3, Z0, L);
    setCmd(q3, Z1, q3, Z1, L);
    setCmd(q3, STAR, q3, STAR, L);
    setCmd(q3, A, q1, A, L); // нашли маркер, шаг влево и снова искать следующий правый немаркированный
    setCmd(q3, B, q1, B, L);
    setCmd(q3, H_SYM, q1, H_SYM, R); // страховочный переход

    // q5: очистка маркеров A/B -> вернуть в Z1/Z0, проход вправо до первого h справа и завершение
    setCmd(q5, A, q5, Z1, R);
    setCmd(q5, B, q5, Z0, R);
    setCmd(q5, Z0, q5, Z0, R);
    setCmd(q5, Z1, q5, Z1, R);
    setCmd(q5, STAR, q5, STAR, R);
    setCmd(q5, H_SYM, qH, H_SYM, H); // достигли правого конца — остановка
}

int main(){
    init_table();

    // подготовка ленты
    string input = "hhh01101hhh"; // можно заменить на любое hhh<код>hhh
    vector<char> tape(1000, 'h');
    int offset = 300;
    for(size_t i=0;i<input.size();++i) tape[offset+i] = input[i];

    int head = offset; // голова в самом начале ленты (на первом 'h')
    int state = q0;

    auto print_segment = [&](int from, int len){
        for(int i=from;i<from+len;i++) cout<<tape[i];
        cout<<"\n";
    };

    cout<<"START: "; print_segment(offset, 20);
    cout<<"\n";

    while(state != qH) {
        int sym = sym_to_id(tape[head]);
        Command cmd = T[state][sym];
        if (cmd.new_state == -1) {
            cerr << "No transition from state "<<state<<" with symbol '"<<tape[head]<<"' (pos="<<head<<")\n";
            break;
        }
        tape[head] = id_to_sym(cmd.write_sym);
        state = cmd.new_state;
        head += cmd.move;
        if (head < 0 || head >= (int)tape.size()) {
            cerr << "Head out of tape range\n";
            break;
        }
    }

    cout<<"\nFINISH: "; print_segment(offset, 30);
    cout<<"\n";

    return 0;
}

/*
---------------------------------------------
Функциональная таблица (короткая записи):
Формат: (состояние, читаемый) -> (новое состояние, записать, движение)

q0, h   -> (q0, h, R)
q0, 0   -> (q0c, 1, R)
q0, 1   -> (q0c, 0, R)

q0c, 0  -> (q0c, 1, R)
q0c, 1  -> (q0c, 0, R)
q0c, h  -> (q1, *, L)
q0c, *  -> (q0c, *, R)

q1, A   -> (q1, A, L)
q1, B   -> (q1, B, L)
q1, 1   -> (q2a, A, R)
q1, 0   -> (q2b, B, R)
q1, h   -> (q5, h, R)

q2a, (0/1/A/B/*) -> (q2a, same, R)
q2a, h -> (q3, 1, L)

q2b, (0/1/A/B/*) -> (q2b, same, R)
q2b, h -> (q3, 0, L)

q3, 0/1/* -> (q3, same, L)
q3, A -> (q1, A, L)
q3, B -> (q1, B, L)

q5, A -> (q5, 1, R)
q5, B -> (q5, 0, R)
q5, 0/1/* -> (q5, same, R)
q5, h -> (qH, h, H)

---------------------------------------------
Функциональная диаграмма (ASCII, упрощённо):

  q0 --(0/1)-> q0c --(0/1 loops)-> q0c --h/*-> q1
                                   |
                                   v
   (влево от '*' искатель)  q1 --(0->B)-> q2b --(вправо)-> ... h -> write 0 -> q3 --(влево)-> q1
                      |--(1->A)-> q2a --(вправо)-> ... h -> write 1 -> q3 --(влево)-> q1
                      |--(A/B -> L)-->
                      '--(h)--> q5 --(A->1, B->0 sweep)-> qH

Диаграмма показывает основной цикл: комплементация (q0/q0c),
повторная пометка правого немаркированного бита (q1), запись копии справа (q2a/q2b),
возврат и повторение (q3). После пометки всех битов выполняется очистка маркеров (q5) и остановка (qH).
---------------------------------------------
*/