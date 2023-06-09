perl dprepro params.in P1_cards_template.txt P1_cards.txt
echo "P1_cards" | DLBreach_Barrier.exe
python parseOut.py P1_cards.out results.out
