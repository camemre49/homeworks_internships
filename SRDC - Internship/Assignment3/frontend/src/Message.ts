export interface Message {
  _id?: string;
  title: string;
  message: string;
  receiver: string;
  sender: string;
  deletedFromSender?: boolean;
  deletedFromReceiver?: boolean;
  date?: Date | string;
}
