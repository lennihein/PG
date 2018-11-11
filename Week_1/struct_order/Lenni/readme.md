Wir geben Adressen aus:

- erste var auf Stackframe
- zweite var auf Stackframe
- lokale var auf neuem Stackframe
- erster Member des Structs
- zweiter Member des Structs
- Basisadresse des Structs

Es werden uint64_t benutzt, um mem alignment hoffentlich als Faktor zu eliminieren.  
Optimierungen schienen keinen Einfluss zu nehmen.