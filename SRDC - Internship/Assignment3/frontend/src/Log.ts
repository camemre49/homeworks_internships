export interface Log {
  userId: string;
  username: string;
  userIp: string;
  method: string;
  userAgent: string;
  route: string;
  responseStatus: number;
  date: Date;
}
