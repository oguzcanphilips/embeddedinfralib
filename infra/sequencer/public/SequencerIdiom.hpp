#ifndef INFRA_SEQUENCER_IDIOM_HPP
#define INFRA_SEQUENCER_IDIOM_HPP

    
#define SEQ_START() sequencer.Load([this]() {
#define SEQ_END()   });

#define SEQ_STEP(step)    sequencer.Step([this]() { step });
#define SEQ_IF(condition) sequencer.If([this] { return condition; });
#define SEQ_ELSE() sequencer.Else();
#define SEQ_ENDIF() sequencer.EndIf();
#define SEQ_WHILE(condition ) sequencer.While([this] { return condition; });
#define SEQ_ENDWHILE() sequencer.EndWhile();



#endif