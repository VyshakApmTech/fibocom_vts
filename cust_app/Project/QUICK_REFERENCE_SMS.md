Incoming SMS
      ↓
Modem Stack
      ↓
fibo_sms_addrxmsghandler() callback
      ↓
Push event to SMS queue
      ↓
sms_task wakes up
      ↓
Read SMS from storage
      ↓
Process content
      ↓
Optional: send reply
      ↓
Delete SMS from storage