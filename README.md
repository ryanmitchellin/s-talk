Name: 
- Ryan Mitchellin (301540810)
- Michael Kwan (301541356)

Project: s-talk

Some concerns:
- We only have 3 threads, which are keyboard sendThread, receiveAndPrintThread, keyboardThread.
- When received a message from other users while typing some message, it output directly resulting in users input looks messy.