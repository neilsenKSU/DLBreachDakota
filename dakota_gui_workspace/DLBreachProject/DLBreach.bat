perl dprepro params.in BigBay_cards_template.txt BigBay_cards.txt
echo "BigBay_cards" | DLBreach_Barrier.exe
python parseOut.py BigBay_cards.out results.out
